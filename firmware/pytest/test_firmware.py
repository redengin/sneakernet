import pytest
import requests

firmware_url = "http://192.168.4.1/api/firmware"



@pytest.mark.skipif(
    "not config.getoption('--firmware')",
    reason="Only run when --firmware is given"
)
def test_firmware(sneakernet):
    response = requests.get(firmware_url)
    assert 200 == response.status_code
    print(response.content)
