#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>

#define I2C_SLAVE_ADDRESS 0x12  // スレーブのI2Cアドレス

int main()
{
    int i2cFile;
    char *devicePath = "/dev/i2c-1";  // I2Cデバイスファイルのパス

    // I2Cデバイスをオープン
    if ((i2cFile = open(devicePath, O_RDWR)) < 0)
    {
        perror("Failed to open the I2C device.\n");
        return 1;
    }

    // スレーブのアドレスを設定
    if (ioctl(i2cFile, I2C_SLAVE, I2C_SLAVE_ADDRESS) < 0)
    {
        perror("Failed to set the I2C slave address.\n");
        return 1;
    }

    // データの送信
    char data = 12;  // 送信するデータ（12）

    while (1)
    {
        if (write(i2cFile, &data, 1) != 1)
        {
            perror("Failed to write to the I2C bus.\n");
            return 1;
        }
        usleep(100000);  // 100ミリ秒待機
    }

    // I2Cデバイスをクローズ
    close(i2cFile);

    return 0;
}