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
* **Librarian** - determines the content of a node
    - see [content management](content_management)
    - see [node control](node_control)
* **Creator** - creates content
* **Publisher** - 
    - authorizes content per [content management](content_management)
* **Courier** - carrier of content
* **Distributor** - deploys a physical device under the control of a **Librarian**


Content Management<a id="content_management"></a>
--------------------------------------------------------------------------------
The **Librarian** dictates the content management of the node.

To reduce the **Librarian's** effort to maintain content, `SneakerNet`
incorporates **Courier's** content per the **Librarian's** controls.

The **Librarian** determines
* which **Publisher** approved content can be on the node
    * node's will disregard **Courier** content from unapproved **Publishers**.
* allow **un-trusted** content (no approved **Publisher**)

Node Control<a id="node_control"></a>
--------------------------------------------------------------------------------
The **Librarian** can also leverage **Couriers** to manage the node. In addition
to publishable content, **Couriers** can bring *signed* node control updates
which:
* add **Publisher** support
* remove **Publisher** support (and the stored content)
* relinquish control to another **Librarian**
