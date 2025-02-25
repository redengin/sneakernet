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

    match hardware:
        case "ESP32":
           ESP32_BIN = "../esp32/build/SneakerNet.bin"
           with open(ESP32_BIN, 'rb') as f:
            assert 200 == requests.put(firmware_url, data=f).status_code
        case _: 
            pytest.fail(f"unsupported hardware type {hardware}")

