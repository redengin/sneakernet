SneakerNet
================================================================================
### [The internet has diluted the sense of local community.](https://en.wikipedia.org/wiki/Internet_influences_on_communities)
> A library outranks any other thing a community can do to benefit its people. It is a never
> failing spring in the desert” – Andrew Carnegie

SneakerNet is a digital version of a [Little Free Library](https://littlefreelibrary.org/) -
a physical place to share content with other local people.

SneakerNet nodes use inexpensive hardware (less than $20 USD). Checkout the
[wiki](https://github.com/redengin/sneakernet/wiki) for details on building your
own SneakerNet node.

### How does SneakerNet work?
Each SneakerNet node is an **open wireless access point** that allows users to
store and remove content.
See the [Concept of Operation](docs/conops.md) for more detail.

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





