FROM pekashy/proxygen-base-image:1.0
RUN ls
RUN apt install curl
RUN mkdir -p /code/_build
COPY . /code
WORKDIR /code/_build
RUN cmake .. && make
EXPOSE 11002
CMD ./balancer_ex
