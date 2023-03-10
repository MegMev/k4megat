

# Why choosing KEY4hep as software build base

Key4hep is a collection of packages/components, which together forms building base for experiment-specific software.
Due to its component-oriented architecture, a prototype application can easily be built on top of it.


## Detector Geometry Management


### DD4hep

DD4hep is the de facto HENP-community standard

-   Experiment-neutral, flexible and extensible
    -   adopted by many experiments (both running and planned)
-   A complete toolsets (optional) to deal with various tasks in HENP experiments
    -   Existing packages: DDCore, DDG4, DDRec, DDAlign etc.
-   Well-designed architecture and interface
-   Code Quality:
    -   stable release (DDCore, DDG4)
    -   proved with production usage in some large experiments

Conclusion: suitable as a long-time solution and worth the investment (time and human)


### REST

-   home-brewed, domain-specific solution
-   mainly focused on TPC, lacking support for other detector types
-   no support/plan for alignment task
-   small user base

Conclusion: worth trying if wanna a quick answer/prototype


## Event Data Model (EDM)


### PODIO and EDM4hep

PODIO/EDM4hep possesses the potential to become a HENP-community standard in the future.

-   Simlple memory-model: Plain-Old-Data members (std::vector of fundamental types on disk)
    -   flat structure and column-oriented
    -   faster I/O
-   Support for vectorization (optional and indirect)
    -   HENP event data by nature is Array of Structure
-   Support for different IO backends
-   Better interface/design:
    -   Value semantics (no pointers needed)
    -   Composition over hierarchy (lightweight means better performance)
    -   Clear object ownership
-   Easily-extensible without loosing above features:
    -   jinja template for new data class
    -   Extension data on existing data class
    -   automatic code-generation
-   Ready-to-use C++ and Python IO backends (thread-safe)
-   (Time)Frame I/O backend in development
    -   in triggerless experiments with high event rates
-   Many planned experiments choose EDM4hep as data model
    -   More third-party tools add support for it, like ACTS

Conclusion: let's just embrace the future (a bright one)


### REST

-   One mono Event class including everything
    -   Event is an aggregate of Tracks
    -   Track is an aggregate  of Hits
    -   Hit is an aggregate of some fundamental types
-   Conventional, ROOT-based with heavy hierarchy:
    -   TObject <- TRestEvent <- TRestGeant4Event
    -   TObject <- TRestEvent <- TRestDetectorHitsEvent
-   Good to have a clear, top overview
-   Not good for early-phase exploratory analysis
-   Not compatible with external tools
-   Major issue: potential performance penalty

Conclusion: Major issue is performance and scalability


## Event Processing Framework

There are many alternatives in the HENP community:

-   REST
-   FairRoot/O2
-   JANA2


### Gaudi

Gaudi is the most flexible one among them, with a large user base and
an eye on the latest software development practices.

Pro:

-   event store as it should be:
    -   data (any format) registered by any previous algorithm is accessible to any later algorithm
    -   A big plus against REST in which a process can only access data from the immediate previous process
-   much cleaner interface/syntax for algorithm development
    -   single-line property definition
    -   single-line event data R/W
    -   value semantics in most cases
-   Python as job configuration language
-   A powerful plugin system
-   task-based MT possible (GaudiHive)
-   functional programming possible (Gaudi::Functional)
-   robust and widely-used: >20 years development and usage

Cons:

-   too many dependencies
-   some deprecated/unused packages inside (for compatibility)

Conclusion:

-   Gaudi is key4hep's the recommended event processing framework.
-   It provides much more than we need and has complicated dependency.
    However, it's bundled inside key4hep Spack repo, which is immediately
    available in USTC server (using CVMFS software deployment mechamism).
    Thus, it turns out to be a pro against REST (an advantage of entering an eco-system).
-   Steep learning curve for software maintainers; in exchange for much simpler interface
    for algorithm developers as well as algorithm users


### REST

Conclusion:

-   Its function and extensible just too limited
-   Does not worth the investment in a long term view


## Software deployment

The deployment of key4hep-based software is pretty simple if using CentOS server.


## Outlook

Key4hep components are more like infrastructure elements, which can be reused by different experiments.
By one-time investment, future projects in USTC can take advantage of the experience and human resources
accumulated and trained in Megat.
This means less maintenance burden, more efficient R&D and decrease of cost in long terms.

Last but not least, Key4hep is still in active development and in the same pace as the latest upgrade of
HENP software stacks. By adopting it in its early stage and entering the community, we're guranteed to keep
pace with the latest development in this field.

\##What's more, we may contribute and become an active member of the coommunity.

