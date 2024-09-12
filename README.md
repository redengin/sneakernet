SneakerNet
================================================================================
SneakerNet is a digital version of a [Little Free Library][lfl] - a physical
place to share content with other local people.

### Low-Cost
SneakerNet is cheaper to produce than a physical [Little Free Library][lfl].
Checkout [creation](https://github.com/redengin/sneakernet/wiki)
for details on building your own SneakerNet node.

## How does SneakerNet work?
Each SneakerNet node is an **open wireless access point** - allowing anyone
to add/remove content.

See the [Concept of Operation](docs/conops.md) for more information.

Software Development
--------------------------------------------------------------------------------
All code is open source under [Apache License Version 2.0](LICENSE).

#### Overview of the repository
* [angular/](angular/) - webapp for users using a browser
* [flutter/](flutter/) - mobile app
* [openapi/sneakernet.yaml](openapi/sneakernet.yaml) - REST interfaces
* [firmware/](firmware/) - embedded software for SneakerNet hardware that implements
    the REST interfaces

### Mobile App Release
* [SneakerNet Android](https://play.google.com/store/apps/details?id=monster.sneakernet)
    on Google Play.



<!-- references -->
[lfl]: https://littlefreelibrary.org/
