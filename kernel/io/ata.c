#include <ata.h>
#include <idt.h>
#include <i686.h>

extern void* ata_isr;

struct identify_data {
  u16  device_type;
  u16  ign0[26];
  char model[40];
  u16  ign1[2];
  u16  capabilities;
  u16  ign2[3];
  u16  field_valid;
  u16  ign3[6];
  u32  max_lba;
  u16  ign4[20];
  u16  command_sets;
  u16  ign5[17];
  u64  max_lba_ext;
  u16  ign6[152];
};

struct ata_info {
  u16  port;
  u8   slave;
  u32 *buffer;

  u16  type;
  u16  sign;
  u16  feats;
  u64  commandSets;
  u64  size; //A: Size in sectors
  char model[41];
};

const char* IDE_MSG[] = {"UNKNOWN", "P/ATA", "SATA", "ATAPI", "SATAPI"};

struct io_dev ioDevices[4];

void ata_wait(u16 port) {
  while (FLAGS_SET(inb(port + ATA_STATUS), 0x80)); //TODO: Not ERR and not DF
}

u16 ata_poll(u16 port) {
  u16 status;
  for (int i = 0; i < 15; i++)
    status = inb(port + ATA_STATUS);
  return status;
}

//READ: https://wiki.osdev.org/ATA_PIO_Mode#IDENTIFY_command
//READ: https://wiki.osdev.org/PCI_IDE_Controller#Detecting_a_PCI_IDE_Controller
errno_t ata_load(struct io_dev *dev) {
  struct ata_info *info = dev->info;
  u16 port = info->port;
  u8 slave = info->slave;
  /*printf("[ata_load] dev=%u, port=%x, master=%x\n", dev->id, port, slave);*/

  outb(port + ATA_DRIVE, slave);

  u8 status = inb(port + ATA_STATUS);
  if (status == 0xFF) return_errno(ENODEV);
  ata_wait(port);

  outb(port + ATA_SECTORCOUNT, 0);
  outb(port + ATA_LBAlo, 0);
  outb(port + ATA_LBAmid, 0);
  outb(port + ATA_LBAhi, 0);

  status = ata_poll(port);
  outb(port + ATA_COMSTAT, ATA_IDENTIFY);
  status = ata_poll(status);
  if (status == 0x00) return_errno(ENODEV);
  ata_wait(port);

  do {
    status = inb(port + ATA_COMSTAT);
  } while ((status & (ATA_STATUS_ERR | ATA_STATUS_DRQ)) == 0);
  if (FLAGS_SET(status, ATA_STATUS_ERR)) { printf("[ata_load] ERR status=%X\n", status); return_errno(ENODEV); } //TODO: Error with status

  u8 type;
  u8 cl = inb(port + ATA_LBAmid),
          ch = inb(port + ATA_LBAmid);
  if (cl == 0x14 && ch == 0xEB) type = ATA_ATAPI;
  else if (cl == 0x69 && ch == 0x96) type = ATA_SATAPI;
  else if (cl == 0 && ch == 0) type = ATA_PATA;
  else if (cl == 0x3C && ch == 0xC3) type = ATA_SATA;
  else type = ATA_UNK;
  if (type == ATA_UNK) { printf("[ata_load] UNKNOWN TYPE ch=%u, cl=%u\n", ch, cl); return_errno(ENODEV); }
  info->type = type;

  struct identify_data identify;
  for (u64 i = 0; i < (sizeof(struct identify_data) / sizeof(u32)); i++)
    ((u32*)&identify)[i] = inw(port + ATA_DATA);

  info->sign = identify.device_type;
  info->feats = identify.capabilities;
  info->commandSets = identify.command_sets;
  info->size = ATA_SECTOR_SIZE * (((info->commandSets & (1 << 26)) != 0) ? identify.max_lba_ext : identify.max_lba);
  for (u64 k = 0; k < 40; k++) info->model[k] = identify.model[k]; info->model[40] = '\0'; //TODO: Model is reading weird
  info->buffer = malloc(ATA_SECTOR_SIZE);

  printf("[ata_load] Detected:\n");
  printf("\t* id=%u, port=%u, slave=%u, type=%s\n", dev->id, info->port, info->slave, IDE_MSG[info->type]);
  printf("\t* sign=%u, feats=%X, commandSets=%X, size=%z\n", info->sign, info->feats, info->commandSets, info->size); 
  printf("\t* model=%s\n", info->model);
  
  return_errno(EOK);
}

errno_t ata_readWrite_(struct io_dev *dev, u32 lba, u16 n, bool write) {
  if (dev->id == (u8)-1) return_errno(ENODEV); //A: Invalid device
  if (n > 0x7F) return_errno(EINVAL); //A: Limit of blocks

  u16 port = ((struct ata_info*)dev->info)->port;
  u8 slave = ((struct ata_info*)dev->info)->slave;
  printf("[ata_readWrite_] dev=%u, lba=%x, n=%u, mode=%s\n", dev->id, lba, n, write ? "WRITE" : "READ");

  //TODO: Choose Master/Slave

  outb(port + ATA_DRIVE, (lba >> 24) | 0xE0);
  outb(port + ATA_SECTORCOUNT, n);
  outb(port + ATA_LBAlo, (u8)lba);
  outb(port + ATA_LBAmid, (u8)(lba >> 8));
  outb(port + ATA_LBAhi, (u8)(lba >> 16));

  if (write) outb(port + ATA_COMSTAT, ATA_WRITERETRY);
  else outb(port + ATA_COMSTAT, ATA_READRETRY);

  u32 *buffer = ((struct ata_info*)dev->info)->buffer;
  if (write) {
    for (u64 block = 0; block < n; block++) { //A: Write
      ata_wait(port); //TODO: Check for errors

      u64 i;
      for (i = 0; i < (ATA_SECTOR_SIZE / sizeof(*buffer)); i++) {
        outw(port, buffer[i]);
        ata_poll(port);
      }
      buffer += i;
    }

    outb(port + ATA_COMSTAT, ATA_CACHECLR);
    ata_wait(port);
  } else for (u64 block = 0; block < n; block++) { //A: Read
    ata_wait(port); //TODO: Check for errors

    u64 i = 0;
    for (i = 0; i < (ATA_SECTOR_SIZE / sizeof(*buffer)); i++) buffer[i] = inw(port);
    buffer += i;
  }

  return_errno(EOK);
}

size_t ata_readWrite(void *buffer, u32 lba, size_t count, struct io_dev *dev, bool write) {
  size_t res = 0;

  u32 *devBuffer = ((struct ata_info*)dev->info)->buffer;
  while (count != 0) {
    size_t _count = MIN(count, ATA_SECTOR_SIZE);
    u16 n = 1;//CEIL(_count, ATA_SECTOR_SIZE); //TODO

    if (write) memcpy(devBuffer, buffer, _count);
    if (ata_readWrite_(dev, lba++, n, write) != EOK) return res;
    if (!write) memcpy(buffer, devBuffer, _count);

    count -= _count;
    buffer += _count;
    res += _count;
  }

  return res;
}

size_t ata_read(void *buffer, size_t lba, size_t count, struct io_dev *dev) {
  if (lba > U32_MAX) { errno = EINVAL; return 0; }
  else return ata_readWrite(buffer, lba, count, dev, false);
}

size_t ata_write(void *buffer, size_t lba, size_t count, struct io_dev *dev) {
  if (lba > U32_MAX) { errno = EINVAL; return 0; }
  else return ata_readWrite(buffer, lba, count, dev, true);
}

u64 ata_init(void) {
  //A: Interruptions
  idt_register(46, &ata_isr, GDT_TYPE_INTERRUPT | GDT_PERM_KERNEL);
  idt_register(47, &ata_isr, GDT_TYPE_INTERRUPT | GDT_PERM_KERNEL);

  u64 count = 0;
  for (u8 i = 0; i < 4; i++) {
    struct ata_info *info = malloc(sizeof(struct ata_info));
    *info = (struct ata_info){
      .port = i < 2 ? ATA0 : ATA1,
      .slave = (i % 2 == 0) ? ATA_MASTER : ATA_SLAVE,
      .buffer = NULL,
    };

    struct io_dev *dev = &ioDevices[i];
    *dev = (struct io_dev){
      .id = i,
      .type = DEV_BLOCK,
      .info = info,
      .read = ata_read,
      .write = ata_write
    };
    sprintf(dev->mount, "dev%u", i); //A: Mounted at "/dev%i"

    if (ata_load(dev) != EOK) dev->id = -1;
    else count++;
  }

  printf("[ata_init] count=%u\n", count);
  return count;
}
