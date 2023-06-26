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

    // 受信と応答の処理
    char buffer[4];  // 受信用のバッファ
    char response[2] = {0xF0, 0x02};  // 応答データ

    while (1)
    {
        // データの受信
        if (read(i2cFile, buffer, 4) != 4)
        {
            perror("Failed to read from the I2C bus.\n");
            return 1;
        }

        // 受信したデータを表示
        printf("Received data: ");
        for (int i = 0; i < 4; i++)
        {
            printf("%02X ", buffer[i]);
        }
        printf("\n");

        // 特定のデータに対して応答を送信
        if (buffer[0] == 0x52 && buffer[1] == 0x00 && buffer[2] == 0x01 && buffer[3] == 0x50)
        {
            // 応答データの送信
            if (write(i2cFile, response, 2) != 2)
            {
                perror("Failed to write to the I2C bus.\n");
                return 1;
            }
            printf("Response sent: F0 02\n");
        }
    }

    // I2Cデバイスをクローズ
    close(i2cFile);

    return 0;
}
