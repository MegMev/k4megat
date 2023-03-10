

# Coding Convention


## Code Tree Structure

-   Prefer short directory name as long as no ambiguity, abbreviated or truncated
-   Prefer lower-case directory name except it's a build interface.
-   `Library` directory should be decomposed into <span class="underline">include</span> and <span class="underline">src</span>
-   `Component` directory should hold source and header together


## Naming

-   Define everything in **megat** namespace, and do not use 'Megat' as a prefix in Class name
-   `Component` export name may use 'Megat' as a prefix, since they are in global space.
-   Important packages may have its own namespace under **megat** e.g. **megat::sim** for the simulation package.
-   Sources and headers should be named after the major class defined in them, e.g. 'TpcDriftAlg.cpp' defines 'TpcDriftAlg'


## Coding style


### Git pre-commit hook and `.clang-format` is used to force a consistent style (adapted from REST)

Make sure clang-format is installed in your development machine.
Don't use USTC server as your develompent machine.
The clang-format version in USTC server is too old that can-t recognize some  config opions.


# Developing

The code contribution from developpers should be managed through pull request.
Developpers need to fork the targeted repository into his/her own account, check [this guide](<https://docs.github.com/en/get-started/quickstart/contributing-to-projects>).
A new branch should be created to keep the his/her personal modifications before pull request.

Before submitting the pull request, the developper should:

-   synchronize his/her local repo with the latest development upstream
-   [rebase](<https://www.atlassian.com/git/tutorials/rewriting-history/git-rebase>) his/her development branch onto the latest development
-   [squash](<https://www.baeldung.com/ops/git-squash-commits>) commit history into one or several commits with meaningful log messages.

These steps are required to have a linear, clear and meaningful log history of the development.


## 0. Fork & Clone

    # clone your forked repo
    git clone https://github.com/youraccount/k4megat.git


## 1. Add the official repo as a new remote in your local clone.

    # use 'upstream' as the official repo's name
      git remote add upstream https://github.com/MegMev/k4megat.git

Now there are two remotes in your local repo:

-   <span class="underline">origin</span> : pointing to youraccount's k4megat repo (i.e. <https://github.com/youraccount/k4megat>)
-   <span class="underline">upstream</span> : pointing to the project's k4megat repo (i.e. <https://github.com/megmev/megRest/>)

<span class="underline">upstream</span> is used to keep updated with latest developments from others
and <span class="underline">origin</span> is used to keep your own development before pull request.


## 2. Create a new branch to record your local developments

For example, 'cool-feature' is the name of the new branch

    # make sure the code is updated
    git switch sim-dev && git pull
    
    # then, create the branch based on main branch
    git checkout -b cool-feature
    
    # setup its push remote to pravite/cool-feature
    git push -u origin cool-feature


## 3. Make changes and keep them as commits

You are now free to modify the code base as your wish (commit, push, new branch, merge). The following commands may be useful in your development.

    git commit
    git push
    git checkout -b sub-cool-feature cool-feature
    git switch cool-feature
    git merge sub-cool-feature
    git log
    git diff
    git restore


## 4. Prepare the pull requset

Ultimately, you are happy with your new feature and ready to publish it.
Before submitting the pull requst, some preparation is requied.
Most importantly, a thorough test should be done in your local machine so that you are confident that the new code functions as expected.


### 4.a Sync with latest updates from other developpers

At the same time as you develop the cool feature, other developpers are also working and may already publish their work before you.
It's important to sync with these latest updates to make sure your modification is compatible with them.

    # sync with upstream repo
    git fetch --all
    
    # rebase your work upon there new development
    git rebase upstream/sim-dev

Most of time, you are working on different parts of the code tree from other developpers, the rebase shall be smooth.
If not, there may be conflicts.
Then, solve all these conficts before proceeding.
You may need to communicate with other developpers to understand their modifications to solve these conflicts.


### 4.b Clean development history by squash commits

It's not unusual to make many small and trivial commits during the development, e.g. backup, quick hot-fix etc.
These commits are meaningless to other developpers and end-users.
To have a clear and meaningful development history, it's required to clean your local commit history by squash.

    # rebase onto main branch again, but this time interactiveli
    git rebase -i upstream/sim-dev

Interactive rebase allows you to squash many trivial commits into several meaningful commits, modify the log message and much more.
Basically, you can re-organize the commit history. There are guides on the terminal showing all behaviors available.

<span class="underline">rebase</span> is a history modification manueover. After rebase, the local branch and the remote tracing branch (e.g. <span class="underline">cool-feature</span> and <span class="underline">private/cool-feature</span>)
may diverge. A [force push](<https://stackoverflow.com/questions/8939977/git-push-rejected-after-feature-branch-rebase>) is needed to keep them coverged (this is mandatory):

    git push --force origin cool-feature


### 5. Create and submit the pull request

Now, you can create the pull request using GitHub website interface.
The *sim-dev* branch of the project account's k4megat repo should be selected as the base repo of the pull request.
The *cool-feature* branch of youraccount's k4megat repo should be selected as the head repo of the pull request.

