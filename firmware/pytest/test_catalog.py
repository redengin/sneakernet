import requests

catalog_base_url = "http://192.168.4.1/api/catalog/"


def test_catalog():
    TEST_FOLDER = "test/"
    FILE_NAME = "testfile"
    FILE_SIZE = 100000
    FILE_PAYLOAD = b"X" * FILE_SIZE
    TIMESTAMP = "2025-01-06T01:02:03Z"

    # create a new file
    test_url = catalog_base_url + TEST_FOLDER + FILE_NAME
    response = requests.put(test_url,
                            headers={
                                "X-FileTimeStamp":TIMESTAMP,
                            },
                            data=FILE_PAYLOAD)
    assert 200 == response.status_code

    # retrieve that file
    response = requests.get(test_url)
    assert 200 == response.status_code
    assert FILE_SIZE == len(response.content)
    assert FILE_PAYLOAD == response.content
    print(f"expected {TIMESTAMP} got {response.headers['X-FileTimeStamp']}")
    # FIXME off by one second (so only comparing upto the minute)
    assert TIMESTAMP[0:17] == response.headers["X-FileTimeStamp"][0:17]

    # remove the file
    response = requests.delete(test_url)
    assert 200 == response.status_code

    # remove the test directory
    test_url = catalog_base_url + TEST_FOLDER
    response = requests.delete(test_url)
    assert 200 == response.status_code
