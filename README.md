<div align="center">
  <img alt="Promptery" src="packaging/icon.svg" width="200" />
</div>

# Promptery

Promptery is a cross-platform (Linux, Mac, Windows) frontend for
[Ollama](https://github.com/ollama/ollama), designed to provide flexible prompt
and context settings during chat interactions with AI models.

## Features

- **Multiple Chat Management**: Seamlessly switch between multiple chat sessions
  to keep track of different conversations simultaneously.
- **System Prompts**: Configure system prompts to guide the AI's behavior and
  tailor responses to specific needs.
- **Content Pages**: Add and manage content pages that can be referenced during
  conversations, providing contextual information for the AI.
- **Decorator Prompts**: Enhance your chat experience by applying decorator
  prompts to customize how the AI presents its responses.
- **External Files Integration**: Easily incorporate external text files into
  chats to expand the AI's knowledge base and contextual understanding.
- **Dynamic Model Switching**: Change the chat model mid-conversation without
  losing context, enabling flexible and adaptive interactions with the AI.
- **Context Customization**: Enable, disable, or rerun former messages to tailor
  the conversation context.

### Outlook

- Image upload
- Multi-step queries
- Additional backends
- Tool usage
- Markdown rendering
- Writing external files

## Build and Run

### Prerequisites

A C++20 development environment with CMake and Qt6 is required to build Promptery.

### Build Instructions

```bash
git clone https://github.com/promptery/promptery
cd promptery

mkdir build
cd build

cmake ..
cmake --build .  # -j <job count> for parallel build
```

### Run

Make sure that Ollama is serving and reachable on your machine. Start Promptery afterwards.

## Contributing

Community contributions are very welcome, especially in the areas of testing and
packaging.

You can contribute via issues and pull requests (PRs).

## License

Promptery is licensed under the [GPL 3.0](LICENSE) license.
