Validation Harness for firmware
================================================================================
Implemented as a poetry pytest.


## Prequisites
[Install poetry](https://python-poetry.org/docs/#installing-with-the-official-installer)

Preferred method
```sh
curl -sSL https://install.python-poetry.org | python3 -
```

[Install Dependencies]
```sh
poetry install
```

# Usage
```sh
poetry run pytest
```

## For pywifi debug info
```sh
poetry run python conftest.py
```
