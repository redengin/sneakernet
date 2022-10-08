Library Architecture
================================================================================
As the intended implementation of a `Library` is for an embedded device, the API
is intentionally constrained. Applications with greater resources can leverage
this minimal API to provide more specialized user experiences (organizing
content, filtering content on parameters like language, etc).

The `MetaEpub` is defined by
https://www.w3.org/publishing/epub3/epub-packages.html#sec-metadata-elem.

```mermaid
classDiagram
    direction LR
    class Library {
        list() list~MetaEpub~
        get(id) Epub
        add(Epub)
    }
    Library --o Epub
    class Epub {
        signature ~optional~
    }
    Epub --|> MetaEpub
    class MetaEpub {
        identifier
        title
        language
    }
    Library -- Configuration
    class Configuration {
        size : size_t
    }
    Configuration -- "0..1" UncontrolledContentConfig
    UncontrolledContentConfig --> ContentConfig
    class ContentConfig {
        size_percentage
        used_percentage()
    }
    Configuration --* "*" ControlledContentConfig
    ControlledContentConfig --> ContentConfig
    class ControlledContentConfig {
        publishers: list
    }
    ControlledContentConfig "1..*" --> "1" Publisher : unique

    class Publisher {
        signature
    }
```
Architectural Requirements
--------------------------------------------------------------------------------
* `Epubs` **shall** be stored "flatly" (aka in a single common folder)
    * Rationale: moving content around on storage is fragile and detracts from
        the primary objective of the `Library` to serve content.
* The total `size_percentage` of all configurations **shall** never exceed 100%.
* Changes to `size_percentage` **shall** only be allowed if `used_percentage()`
    does not exceed the new limit.
* `ControlledContentConfig` publishers **shall** be unique (i.e. the same
    publisher can not be used within another `ControlledContentConfig`)
    * Rationale: allowing publishers to be aliased across
        `ControlledContentConfigs` would make `size_percentage` ambiguous.

Library Interface Requirements
--------------------------------------------------------------------------------
* Upon `list()`, `Library` **shall** return a listing of content metadata.
* Upon `get()`, `Library` **shall** return the specified ebook.
* Upon `add()`, `Library` **shall** only accept the ebook if the `Configuration`
    supports it's additional storage.
    * (`used_percentage` + new addition) **must** be less than `size_percentage`