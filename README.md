# SPDIF audio output for Android devices

This circuit uses the general purpose microcontroller PIC32MX250 to read audio data from an Android device and to output a corresponding SPDIF audio stream over an SPI port.

The SPDIF bitstream is generated in software. As a consequence, no special encoder chip is needed. An optical SPDIF transmitter can directly connected to the SPI/I2S port of the microcontroller.
