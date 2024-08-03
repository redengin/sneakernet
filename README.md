SneakerNet
================================================================================
> A library outranks any other thing a community can do to benefit its people. It is a never failing spring in the desert” – Andrew Carnegie

### [The internet has diluted the sense of local community.](https://en.wikipedia.org/wiki/Internet_influences_on_communities)
SneakerNet is a digital version of a [Little Free Library](https://littlefreelibrary.org/).

SneakerNet nodes present themselves as **wifi access points** providing locally curated content. See [Concept](docs/conops.md) for more details.

Usage Implementations:
* [Browser](angular/sneakernet/) - serves a web application that can send/delete content.
    * iPhone doesn't support the wifi api needs of SneakerNet, and therefore can only
        be supported by a web application.
* [Android App](flutter/)  
    * raises notifications when there are SneakerNet nodes in your vicinity.
    * automatically curates objectionable (flagged) content.
        * deletes files you've flagged from SneakerNet nodes you come in contact with.
    * Get it from [Google Play](https://play.google.com/store/apps/details?id=monster.sneakernet)


Providing a SneakerNet Node
--------------------------------------------------------------------------------
Anyone can build a SneakerNet node.

SneakerNet nodes use inexpensive hardware (less than $20 USD). Checkout the
[wiki](https://github.com/redengin/sneakernet/wiki) for details.


Software Development
--------------------------------------------------------------------------------
All code is open source under [Apache License Version 2.0](LICENSE).

SneakerNet firmware is signed by a private key (not published).
