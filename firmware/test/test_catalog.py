import pytest
import requests

catalog_base_url = "http://192.168.4.1/api/catalog/"


def test_catalog(sneakernet):
    # test root file creation/deletion
    # upload a file
    TEST_FILENAME = "testing..."
    assert 200 == createFile(TEST_FILENAME)

    #TODO validate file

    # remove the file
    assert 200 == deleteFile(TEST_FILENAME)

# test directory file creation/deletion
    # create a directory
    TEST_DIRECTORY = "test/"
    assert 200 == requests.put(catalog_base_url + TEST_DIRECTORY).status_code

    # upload a file into directory
    assert 200 == createFile(TEST_DIRECTORY + TEST_FILENAME)

    #TODO validate file

    # remove the file
    assert 200 == deleteFile(TEST_DIRECTORY + TEST_FILENAME)

    # remove a directory
    TEST_DIRECTORY = "test/"
    assert 200 == requests.delete(
        catalog_base_url + TEST_DIRECTORY).status_code

# test changing file title
    # upload a file
    assert 200 == createFile(TEST_FILENAME)

    # change title
    assert 200 == requests.put(catalog_base_url + TEST_FILENAME,
                               params={
                                   "title": "this is a test"
                               }).status_code

    #TODO validate title change

    # remove the file
    assert 200 == deleteFile(TEST_FILENAME)


# test utilities
# ===============================================================================
def createFile(filepath: str):
    return requests.put(catalog_base_url + filepath,
                        headers={
                            "X-timestamp": "2025-01-06T01:02:03Z"
                        },
                        data="0"
                        ).status_code


def deleteFile(filepath: str):
    return requests.delete(catalog_base_url + filepath).status_code
