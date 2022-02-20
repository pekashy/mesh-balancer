FROM pekashy/proxygen-base-image:1.0
RUN apt install curl -y
RUN mkdir -p /code/_build
COPY . /code
WORKDIR /code/_build
RUN cmake .. && make
CMD ./proxygen_proxy