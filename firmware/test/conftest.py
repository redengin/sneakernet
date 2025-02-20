import pytest

import pywifi

@pytest.fixture
def sneakernet():
    wifi = pywifi.PyWiFi()

    # requires netdev group access
    try:
        iface = wifi.interfaces()[0]
    except PermissionError as e:
        print(e)
        pytest.fail("make sure user is part of 'netdev' group")

    # connect to SneakerNet node
    profile = pywifi.Profile()
    profile.ssid = "SneakerNet"
    profile.auth = pywifi.const.AUTH_ALG_OPEN
    profile.key = pywifi.const.AKM_TYPE_NONE

    yield

    # FIXME requires root access
    # iface.disconnect()