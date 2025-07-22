FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
  curl wget git build-essential python3 python3-pip jq unzip \
  nodejs npm ca-certificates gnupg software-properties-common libssl-dev libffi-dev \
  && apt-get clean

RUN apt-get update && apt-get install -y \
    curl wget unzip gnupg lsb-release \
    git python3 python3-pip python3-setuptools python-is-python3 \
    build-essential software-properties-common ca-certificates \
    mingw-w64 cmake openjdk-21-jdk \
    nodejs npm \
    && rm -rf /var/lib/apt/lists/*
    
RUN update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix \
    && update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

# Install SCons
RUN pip install --no-cache-dir --break-system-packages scons==4.8

# Define Godot version and paths
ENV GODOT_VERSION=4.4.1
ENV GODOT_TEMPLATES_VERSION=4.4.1.stable
ENV GODOT_TEMPLATES_PATH=/usr/local/share/godot/export_templates/${GODOT_TEMPLATES_VERSION}
ENV EXPORT_TEMPLATES_ZIP=/tmp/export_templates.zip

# Download and install Godot editor/headless binary
#RUN wget -qO /tmp/godot.zip \
#      https://github.com/godotengine/godot/releases/download/${GODOT_VERSION}-stable/Godot_v${GODOT_VERSION}-stable_linux.x86_64.zip && \
#    unzip /tmp/godot.zip -d /usr/local/bin && \
#    mv /usr/local/bin/Godot_v${GODOT_VERSION}-stable_linux.x86_64 /usr/local/bin/godot && \
#    chmod +x /usr/local/bin/godot && \
#    rm /tmp/godot.zip

# Download and extract Godot export templates
#RUN mkdir -p "$GODOT_TEMPLATES_PATH" && \
#    wget -qO "$EXPORT_TEMPLATES_ZIP" \
#      https://github.com/godotengine/godot/releases/download/${GODOT_VERSION}-stable/Godot_v${GODOT_VERSION}-stable_export_templates.tpz && \
#    unzip "$EXPORT_TEMPLATES_ZIP" -d /tmp/templates && \
#    mv /tmp/templates/templates/* "$GODOT_TEMPLATES_PATH" && \
#    rm -rf /tmp/templates "$EXPORT_TEMPLATES_ZIP"

# Install EMSDK (Emscripten)
ENV EMSDK_VERSION=3.1.64
ENV EMSDK_ROOT=/opt/emsdk
RUN git clone https://github.com/emscripten-core/emsdk.git ${EMSDK_ROOT} && \
    cd ${EMSDK_ROOT} && \
    ./emsdk install ${EMSDK_VERSION} && \
    ./emsdk activate ${EMSDK_VERSION}
ENV PATH="${EMSDK_ROOT}:${EMSDK_ROOT}/upstream/emscripten:${PATH}"

# Install Solana CLI
ENV SOLANA_VERSION=2.2.20
ENV SOLANA_BIN_DIR=/usr/local/bin
RUN curl -L https://github.com/anza-xyz/agave/releases/download/v${SOLANA_VERSION}/solana-release-x86_64-unknown-linux-gnu.tar.bz2 \
      -o /tmp/solana.tar.bz2 && \
    tar -xjf /tmp/solana.tar.bz2 -C /tmp && \
    mv /tmp/solana-release/bin/* ${SOLANA_BIN_DIR}/ && \
    chmod +x ${SOLANA_BIN_DIR}/solana/* && \
    rm -rf /tmp/solana.tar.bz2 /tmp/solana-release

# Remove conflicting system Node.js (v12) before installing newer version
#RUN apt-get purge -y nodejs libnode-dev && \
#    curl -fsSL https://deb.nodesource.com/setup_22.x | bash - && \
#    apt-get install -y nodejs
#    npm install -g npm@11 puppeteer


# Install Android SDK
RUN mkdir -p /opt/android-sdk && \
    cd /opt && \
    curl -fsSL https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip -o cmdline-tools.zip && \
    unzip cmdline-tools.zip -d /opt/android-sdk && \
    rm cmdline-tools.zip && \
    mkdir -p /opt/android-sdk/cmdline-tools/latest && \
    mv /opt/android-sdk/cmdline-tools/* /opt/android-sdk/cmdline-tools/latest || true

ENV ANDROID_SDK_ROOT="/opt/android-sdk"
ENV PATH="$PATH:$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/platform-tools"

# Install Android SDK tools
RUN ls $ANDROID_SDK_ROOT/cmdline-tools/latest && \
    yes | ${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin/sdkmanager --sdk_root="$ANDROID_SDK_ROOT" --licenses && \
    ${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin/sdkmanager --sdk_root="$ANDROID_SDK_ROOT" \
        "platform-tools" \
        "build-tools;30.0.3" \
        "platforms;android-29" \
        "cmdline-tools;latest" \
        "cmake;3.10.2.4988404" \
        "ndk;23.2.8568313"

CMD ["/bin/bash"]

ENV JAVA_HOME="/usr/lib/jvm/java-21-openjdk-amd64"