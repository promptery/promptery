<div align="center">
  <img alt="Promptery" src="packaging/icon.svg" width="200" />
</div>

# Promptery

Promptery is a cross-platform (Linux, Mac, Windows) frontend for
[Ollama](https://github.com/ollama/ollama), designed to provide flexible prompt
and context settings during chat interactions with AI models.


## Features

- **Multiple Chat Management**: Switch between multiple chat sessions effortlessly.
- **System Prompts**: Configure prompts to guide the AI's behavior.
- **Content Pages**: Manage content pages for referencing during conversations.
- **Decorator Prompts**: Customize how the AI presents its responses.
- **External Files Integration**: Incorporate external text files into chats.
- **Dynamic Model Switching**: Change models mid-conversation without losing context.
- **Context Customization**: Tailor conversation context by enabling, disabling, or rerunning former messages.

### Screenshots

<div style="display: flex;">
  <div style="width: 50%; box-sizing: border-box;" align="center">
    <a href="doc/screenshots/First start.png">
      <img alt="Promptery - First Start" src="doc/screenshots/First start.png" width="300"/>
    </a>
    <div style="width: 400px;">
    <b>First Start:</b> Promptery's initial user interface upon first launch.
    </div>
  </div>

  <div style="width: 50%; box-sizing: border-box;" align="center">
    <a href="doc/screenshots/Role play with external file.png">
      <img alt="Promptery - Roleplay Example" src="doc/screenshots/Role play with external file.png" width="300"/>
    </a>
    <div style="width: 400px;">
    <b>Roleplay Example:</b> Demonstrating a roleplay scenario with characters like a pirate, butler, and coding assistant using an external file for additional context.
    </div>
  </div>
</div>

### Outlook

- Image upload
- Multi-step queries
- Additional backends
- Tool usage
- Markdown rendering
- Writing external files


## Development Status

Promptery is a hobby project in its early stages. It is not ready for production
and may lose stored data. Expect bugs and lacking error handling. Save and
backup regularly. Currently all user data is stored in a settings directory that
adheres to your platform's standard location for such files.


## Download Prebuild Version

Prebuild releases for Mac (Apple Silicon) and Windows are available on [GitHub](https://github.com/promptery/promptery/releases).


## Build and Run

### Prerequisites

A C++20 development environment with CMake and Qt6 is required to build Promptery.

### Build Instructions

```bash
git clone https://github.com/promptery/promptery
cd promptery

mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .  # -j <job count> for parallel build
```

### Run

Ensure Ollama is reachable and serving at least one model. Start Promptery afterwards.


## Contributing

Community contributions are welcome, especially in testing and packaging. Contribute via issues and pull requests.


## License

Promptery is licensed under the [GPL-3.0](LICENSE) license.
