FROM pekashy/proxygen-base-image:1.0
EXPOSE 11002
EXPOSE 11003
RUN mkdir log
RUN git clone https://github.com/jbeder/yaml-cpp.git && cd yaml-cpp && mkdir build && cd build && cmake .. && make -j && make install
RUN apt update && apt install -y curl libspdlog-dev libgtest-dev
RUN mkdir -p /code/_build
COPY . /code
RUN cmake /code && make -j
CMD ./balancer_ex code/config.yaml 5
