# shellcheck disable=SC2046
docker stop $(docker ps -q)
docker build . -t "pekashy/mesh-balancer"
docker run -it -p 11002:11002 pekashy/mesh-balancer:latest