FROM pekashy/proxygen-base-image:1.0
EXPOSE 11002
EXPOSE 11003
RUN mkdir log
RUN apt install -y curl libspdlog-dev libgtest-dev
RUN mkdir -p /code/_build
COPY . /code
RUN cmake /code && make -j
CMD ./balancer_ex
