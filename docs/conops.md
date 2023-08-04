Concept of Operations
================================================================================
<!-- main scenario -->
When a person enters into the physical space of a `SneakerNet` node, they will
be able to access the content stored on that node.

Human Factors
--------------------------------------------------------------------------------
* Awareness - the person must be made aware that the content is available
    * primarily this can be accomplished by an application installed on the
        phone that recognizes `SneakerNet` nodes
    * `SneakerNet` nodes can also publish themselves as a wifi connection
        - attracting those looking for internet connectivity

Roles
================================================================================
* **Reader** - consumer of content
* **Courier** - carrier of content
    - **Readers** that use the `mobile app`, will automatically become couriers.  
        In this way, content is automatically distributed.
* **Librarian** - restricts the content of a node
    - see [Content Management](content_management)
* **Creator** - creates content
* **Publisher** - 
    - signs content per [content management](content_management)
* **Distributor** - deploys a physical device
    - can choose a Librarian


Content Management<a id="content_management"></a>
--------------------------------------------------------------------------------
Without a **Librarian** the content of a node is **unrestricted**.

The **Librarian** determines
* which **Publishers** `epub` content can be on the node
    - `SneakerNet` signatures of the `epub`
* allow **un-trusted** content on the node

Content can be **deleted** by any user. This empowers the community to manage
the content organically.

Node Configuration Control<a id="node_configuration"></a>
--------------------------------------------------------------------------------
* **Firmware**
    * The **Firmware** is securely signed.
    * The `mobile app` will automatically update the node's firmware.
* **Librarian Configuration**
    * The **Librarian Configuration** is securely signed.
    * The `mobile app` will carry the **Librarian Configurations** of the nodes
        it has come in contact with.
        * When the node comes into contact with a node, it will attempt to
            update the **Librarian Configuration**.
