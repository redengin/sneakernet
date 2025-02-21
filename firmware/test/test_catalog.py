import pytest
import requests

catalog_base_url = "http://192.168.4.1/api/catalog/"


def test_catalog(sneakernet):
    # upload a file
    TEST_FILENAME = "testing..."
    response = requests.put(catalog_base_url + TEST_FILENAME,
                            headers={
                                "X-timestamp":"2025-01-06T01:02:03Z"
                            },
                            data="0",
                            timeout=10)
    assert 200 == response.status_code

    # delete the test file
    response = requests.delete(catalog_base_url + TEST_FILENAME,
                            timeout=10)
    assert 200 == response.status_code


