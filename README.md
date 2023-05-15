### Had to make some changes to the gg setup

- Changed the hardcoded thunk size: https://github.com/StanfordSNR/gg/issues/47#issuecomment-666840576 (don't forget to change the actual function config too!)
- Changed to Ubuntu 20.04
- Removed python-minimal from the dependencies, not releant in Ubuntu 20.04
- Changed function runtime to python 3.9 as 3.6 is not supported anymore
- Regenerated protobuf files
- Using build-env
- added ENV DEBIAN_FRONTEND=noninteractive to Dockerfile

Then follow the setup shown in the official [readme](https://github.com/StanfordSNR/gg/blob/master/docker/README.md)