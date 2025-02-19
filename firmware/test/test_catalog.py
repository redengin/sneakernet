import pytest
import requests

catalog_base_url = "http://192.168.4.1/api/catalog/"

def test_catalog(sneakernet):
    assert True