FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
  curl wget git build-essential python3 python3-pip jq unzip \
  nodejs npm ca-certificates gnupg software-properties-common libssl-dev libffi-dev \
  && apt-get clean

# Install SCons
RUN pip3 install --no-cache-dir scons==4.8

# Download Godot headless/editor Linux binary (v4.4.1 stable)
ENV GODOT_VERSION=4.4.1
RUN wget -qO godot.zip \
    https://github.com/godotengine/godot/releases/download/${GODOT_VERSION}-stable/Godot_v${GODOT_VERSION}-stable_linux.x86_64.zip && \
    unzip godot.zip -d /usr/local/bin && \
    mv /usr/local/bin/Godot_v${GODOT_VERSION}-stable_linux.x86_64 /usr/local/bin/godot && \
    chmod +x /usr/local/bin/godot && \
    rm godot.zip

# Install emsdk
ENV EMSDK_VERSION=3.1.64
RUN git clone https://github.com/emscripten-core/emsdk.git /emsdk && \
  cd /emsdk && ./emsdk install ${EMSDK_VERSION} && ./emsdk activate ${EMSDK_VERSION}
ENV PATH="/emsdk:/emsdk/upstream/emscripten:${PATH}"

# Install Solana CLI
RUN curl -sSfL https://solana-install.solana.workers.dev | bash
ENV PATH="/root/.local/share/solana/install/active_release/bin:${PATH}"

# Remove conflicting system Node.js (v12) before installing newer version
RUN apt-get purge -y nodejs libnode-dev && \
    curl -fsSL https://deb.nodesource.com/setup_22.x | bash - && \
    apt-get install -y nodejs && \
    npm install -g npm@11 puppeteer

WORKDIR /workspace
CMD ["/bin/bash"]
