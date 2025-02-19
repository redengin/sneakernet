import pytest

import pywifi

@pytest.fixture
def sneakernet():
    wifi = pywifi.PyWiFi()

    # FIXME requires root access
    # iface = wifi.interfaces()[0]

    # connect to SneakerNet node
    profile = pywifi.Profile()
    profile.ssid = "SneakerNet"
    profile.auth = pywifi.const.AUTH_ALG_OPEN

    yield

    # FIXME requires root access
    # iface.disconnect()