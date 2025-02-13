Firmware
================================================================================
Embedded software for the SneakerNet node.


SSL Signatures
--------------------------------------------------------------------------------
### SneakerNet node firmware is signed to protect against malicious updates.  
**The firmware key is not shared.**

The firmware signature follows the
[ESP32 Specification](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/security/secure-boot-v1.html#secure-boot-image-signing-algorithm).

### SneakerNet HTTPS uses a self-signed cert
To ensure admin credentials security, admin access uses HTTPS.

The https cert follows the
[ESP32 Specification](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/security/secure-boot-v1.html#secure-boot-image-signing-algorithm).
