Build the Docker Image
================================================================================
```sh
docker build -t angular/alpine .
```

Create an Angular Project
--------------------------------------------------------------------------------
```sh
docker run --rm -it \
    --user $(id -u):$(id -g) \
    -v $PWD:$PWD \
    -w $PWD \
    --env HOME=$PWD \
    angular/alpine \
        ng new --skip-install <app dir>
```