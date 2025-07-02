import pytest
import requests


def test_catalog(sneakernet):

    # test the root folder
    FILENAME = "testfile.test"
    FILEDATA = b"this is a test file"
    assert (200 == createPath(FILENAME, FILEDATA))
    (status_code, content) = getFile(FILENAME)
    assert (200 == status_code)
    assert (FILEDATA == content)
    deletePath(FILENAME)

    # test a subfolder
    FOLDER = "testFolder/"
    assert (200 == createPath(FOLDER + FILENAME, FILEDATA))
    (status_code, content) = getFile(FOLDER + FILENAME)
    assert (200 == status_code)
    assert (FILEDATA == content)
    deletePath(FOLDER + FILENAME)
    deletePath(FOLDER)

    # test custom file icon
    ICONDATA = b"this is a file icon"
    assert (200 == createPath(FILENAME, FILEDATA))
    assert (200 == setIcon(FILENAME, ICONDATA))
    (status_code, content) = getIcon(FILENAME)
    assert (200 == status_code)
    assert (ICONDATA == content)
    assert (200 == deleteIcon(FILENAME))
    (status_code, content) = getIcon(FILENAME)
    assert (404 == status_code)
    deletePath(FILENAME)

    # test custom file title
    TITLE = "this is a title"
    assert (200 == createPath(FILENAME, FILEDATA))
    assert (200 == setTitle(FILENAME, TITLE))
    (status_code, content) = contents()
    assert (200 == status_code)
    assert (FILENAME in content)
    assert (TITLE == content.get(FILENAME).get("title"))
    assert (200 == deleteTitle(FILENAME))
    (status_code, content) = contents()
    assert (200 == status_code)
    assert (None == content.get(FILENAME).get("title"))
    deletePath(FILENAME)

    # test custom file title and icon are removed with file
    assert (200 == createPath(FILENAME, FILEDATA))
    assert (200 == setTitle(FILENAME, TITLE))
    assert (200 == setIcon(FILENAME, ICONDATA))
    assert (200 == deletePath(FILENAME))
    (status_code, content) = getIcon(FILENAME)
    assert (404 == status_code)

    # test custom folder icon
    assert (200 == createPath(FOLDER))
    assert (200 == setIcon(FOLDER, ICONDATA))
    (status_code, content) = getIcon(FOLDER)
    assert (200 == status_code)
    assert (ICONDATA == content)
    assert (200 == deleteIcon(FOLDER))
    (status_code, content) = getIcon(FOLDER)
    assert (404 == status_code)
    deletePath(FOLDER)

    # test folder icon removed when folder removed
    (status_code, content) = contents(FOLDER)
    assert (200 == createPath(FOLDER))
    assert (200 == setIcon(FOLDER, ICONDATA))
    assert (200 == deletePath(FOLDER))
    (status_code, content)=getIcon(FOLDER)
    assert (404 == status_code)


# test utilities
# ===============================================================================
base_url = "http://192.168.4.1"
uri_sync = "/api/catalog.content"
uri_content = "/api/catalog/"
uri_icon = "/api/catalog.icon/"
uri_title = "/api/catalog.title/"


def contents(filepath: str = "/"):
    req = requests.get(base_url + uri_content + filepath)
    data = req.json() if (200 == req.status_code) else req.content
    return (req.status_code, data)


def createPath(filepath: str, data: bytes = None):
    return requests.put(base_url + uri_content + filepath,
                        headers={"X-timestamp": "2025-01-06T01:02:03Z"},
                        data=data
                        ).status_code


def getFile(filepath: str):
    req = requests.get(base_url + uri_content + filepath)
    return (req.status_code, req.content)


def deletePath(filepath: str):
    return requests.delete(base_url + uri_content + filepath).status_code


def setIcon(filepath: str, data: bytes):
    return requests.put(base_url + uri_icon + filepath, data=data).status_code


def getIcon(filepath: str):
    req = requests.get(base_url + uri_icon + filepath)
    return (req.status_code, req.content)


def deleteIcon(filepath: str):
    return requests.delete(base_url + uri_icon + filepath).status_code


def setTitle(filepath: str, data: str):
    return requests.put(base_url + uri_title + filepath, params={"title": data}).status_code


def deleteTitle(filepath: str):
    return requests.delete(base_url + uri_title + filepath).status_code
