import pytest

import pywifi

import time

@pytest.fixture
def sneakernet():

    # SneakerNet node profile
    profile = pywifi.Profile()
    profile.ssid = "SneakerNet"
    profile.auth = pywifi.const.AUTH_ALG_OPEN
    profile.key = pywifi.const.AKM_TYPE_NONE

    wifi = pywifi.PyWiFi()
    iface = None
    try:
        iface = wifi.interfaces()[1]
    except PermissionError as e:
        print(e)
        pytest.fail("make sure user is part of 'netdev' group")

    # connect to sneakernet
    p = iface.add_network_profile(profile) 
    iface.connect(p)
    time.sleep(1)
    while True:
        status = iface.status()
        if status in [pywifi.const.IFACE_CONNECTING, pywifi.const.IFACE_SCANNING]:
            continue
        break
    assert iface.status() == pywifi.const.IFACE_CONNECTED

    # perform the test
    yield

    # disconnect from sneakernet
    iface.disconnect()


if __name__ == "__main__":
    ''' provide info about pywifi
    '''
    wifi = pywifi.PyWiFi()
    iface = wifi.interfaces()[1]
    print(f"Using wifi: {iface.name()}")

    print("Profiles:")
    for p in iface.network_profiles():
        print(f"\t{vars(p)}")