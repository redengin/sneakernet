import pytest
import requests
import json

firmware_url = "http://192.168.4.1/api/firmware"

@pytest.mark.skipif(
    "not config.getoption('--firmware')",
    reason="Only run when --firmware is given")
def test_firmware(sneakernet):
    response = requests.get(firmware_url)
    assert 200 == response.status_code

    hardware = response.json()['hardware']
    version = response.json()['version']

    # send the firmware
    match hardware:
        case "ESP32":
           ESP32_BIN = "../esp32/build/SneakerNet.bin"
           with open(ESP32_BIN, 'rb') as f:
            response = requests.put(firmware_url, data=f)
        case _: 
            pytest.fail(f"unsupported hardware type {hardware}")

    # handle response (403 just means the version isn't newer)
    assert response.status_code in [200, 403]