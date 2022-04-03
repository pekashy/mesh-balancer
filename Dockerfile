FROM pekashy/proxygen-base-image:1.0
RUN git clone https://github.com/google/glog.git
RUN cd glog && cmake -S . -B build -G "Unix Makefiles" && cmake --build build && cmake --build build --target install
RUN apt install curl libgoogle-glog-dev -y
RUN mkdir -p /code/_build
COPY . /code
WORKDIR /code/_build
RUN cmake .. && make
CMD ./proxygen_proxy