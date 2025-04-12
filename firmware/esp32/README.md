<!-- End of life
================================================================================
This implementation is a prototype using the ESP-IDF. Future work will focus on
the Rest implementation to support more hardware
-->

# Development environment
Using the vscode [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension).

**Currently using ESP-IDF v5.4.1**

## Using VSCODE
You need to initialize environment variables via esp-idf's **export.sh** before
opening the folder.

For convenience, add the following to your **.bashrc**
```sh
# provide shell for ESP IDF
ESP_IDF_VERSION="v5.4.1"
if [ -f "$HOME/opt/espressif/$ESP_IDF_VERSION/esp-idf/export.sh" ]; then
    alias esp-idf='. "$HOME/opt/espressif/$ESP_IDF_VERSION/esp-idf/export.sh"'
fi
```
Before you begin development run **esp-idf** - to provide the paths to the source and tools.

By running **esp-idf**, you'll also be able to use *idf.py* from the command-line.
