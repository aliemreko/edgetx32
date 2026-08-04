/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/storage.h"
#include "hal/fatfs_diskio.h"
#include "board.h"
#include "esp32_gpio.h"

#if defined(ESP_PLATFORM)
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_master.h"
#endif

static bool s_present = false;

#if defined(ESP_PLATFORM)
static sdmmc_card_t* s_card = nullptr;
#endif

static DSTATUS sd_initialize(BYTE)
{
#if defined(ESP_PLATFORM)
  if (s_card) return 0;

  spi_bus_config_t bus_cfg = {};
  bus_cfg.mosi_io_num = esp32_gpio_num(SD_PIN_MOSI);
  bus_cfg.miso_io_num = esp32_gpio_num(SD_PIN_MISO);
  bus_cfg.sclk_io_num = esp32_gpio_num(SD_PIN_SCLK);
  bus_cfg.quadwp_io_num = -1;
  bus_cfg.quadhd_io_num = -1;
  spi_bus_initialize((spi_host_device_t)SD_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot = (spi_host_device_t)SD_SPI_HOST;

  sdspi_device_config_t slot = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot.gpio_cs = (gpio_num_t)esp32_gpio_num(SD_PIN_CS);
  slot.host_id = (spi_host_device_t)SD_SPI_HOST;

  esp_vfs_fat_sdmmc_mount_config_t mount = {
    .format_if_mount_failed = false,
    .max_files = 8,
    .allocation_unit_size = 16 * 1024
  };

  // EdgeTX uses FatFs diskio directly; still probe card presence via SDSPI
  esp_err_t err = esp_vfs_fat_sdspi_mount("/sd", &host, &slot, &mount, &s_card);
  s_present = (err == ESP_OK);
  return s_present ? 0 : STA_NOINIT;
#else
  s_present = true;
  return 0;
#endif
}

static DSTATUS sd_deinit(BYTE)
{
#if defined(ESP_PLATFORM)
  if (s_card) {
    esp_vfs_fat_sdcard_unmount("/sd", s_card);
    s_card = nullptr;
  }
#endif
  s_present = false;
  return 0;
}

static DSTATUS sd_status(BYTE)
{
  return s_present ? 0 : STA_NODISK;
}

static DRESULT sd_read(BYTE, BYTE* buff, DWORD sector, UINT count)
{
#if defined(ESP_PLATFORM)
  if (!s_card) return RES_NOTRDY;
  return sdmmc_read_sectors(s_card, buff, sector, count) == ESP_OK ? RES_OK : RES_ERROR;
#else
  (void)buff; (void)sector; (void)count;
  return RES_OK;
#endif
}

static DRESULT sd_write(BYTE, const BYTE* buff, DWORD sector, UINT count)
{
#if defined(ESP_PLATFORM)
  if (!s_card) return RES_NOTRDY;
  return sdmmc_write_sectors(s_card, buff, sector, count) == ESP_OK ? RES_OK : RES_ERROR;
#else
  (void)buff; (void)sector; (void)count;
  return RES_OK;
#endif
}

static DRESULT sd_ioctl(BYTE, BYTE cmd, void* buff)
{
#if defined(ESP_PLATFORM)
  if (!s_card) return RES_NOTRDY;
  switch (cmd) {
    case CTRL_SYNC: return RES_OK;
    case GET_SECTOR_COUNT:
      *(DWORD*)buff = (DWORD)s_card->csd.capacity;
      return RES_OK;
    case GET_SECTOR_SIZE:
      *(WORD*)buff = 512;
      return RES_OK;
    default: return RES_PARERR;
  }
#else
  (void)cmd; (void)buff;
  return RES_OK;
#endif
}

static const diskio_driver_t s_sd_drv = {
  .initialize = sd_initialize,
  .deinit = sd_deinit,
  .status = sd_status,
  .read = sd_read,
  .write = sd_write,
  .ioctl = sd_ioctl,
};

void storageInit()
{
  fatfsRegisterDriver(&s_sd_drv, 0);
}

void storageDeInit()
{
  fatfsUnregisterDrivers();
}

void storagePreMountHook() {}

bool storageIsPresent() { return s_present; }

const diskio_driver_t* storageGetDefaultDriver() { return &s_sd_drv; }
