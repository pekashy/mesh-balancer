FROM pekashy/proxygen-base-image:1.0
RUN ls
RUN apt install -y curl libspdlog-dev
RUN mkdir -p /code/_build
COPY . /code
RUN cmake /code && make -j
EXPOSE 11002
EXPOSE 11003
RUN mkdir log
CMD ./balancer_ex
