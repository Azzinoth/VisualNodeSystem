<div align="center">
	<h1> Visual Node System </h1>
	<img src="https://github.com/Azzinoth/VisualNodeSystem/actions/workflows/Build.yml/badge.svg" alt="Build Status">
	<img src="https://github.com/Azzinoth/VisualNodeSystem/actions/workflows/ContinuousIntegration.yml/badge.svg" alt="Continuous Integration">
</div>

This library provides a powerful framework for creating and managing visual node systems. These are typically used in graphical programming environments, game logic builders, and AI behavior tree editors.

Master branch is free of tests to maintain its lightweightness. All test-related stuff (except GitHub Actions) is located in the [Tests Branch](https://github.com/Azzinoth/VisualNodeSystem/tree/tests)

## Key Features

- **Unchanged Dear Imgui**: This project does not depend on modifications to Dear Imgui.

- **Zoom**: Zoom functionality enables detailed viewing and efficient navigation. The font system ensures precise text rendering at various zoom levels.
<div align="center">
    <img src="https://github.com/Azzinoth/VisualNodeSystem/blob/media/Zoom.gif" width="60%">
</div>

- **Reroute Nodes**: Reroute nodes offer enhanced organization for clearer, more readable visual graphs, enabling flexible customization of connection paths. Also they simplify debugging by making connections traceable. To add reroute node just double click on connection.
<div align="center">
    <img src="https://github.com/Azzinoth/VisualNodeSystem/blob/media/RerouteNodes.gif" width="60%">
</div>

- **Group Comments**: Improve clarity and organization within the visual node-based interface by allowing users to encircle and highlight node collections, signifying their combined functionality.
<div align="center">
    <img src="https://github.com/Azzinoth/VisualNodeSystem/blob/media/Group Comments.png" width="100%">
</div>

- **Flexible Socket Management**: The library offers functionalities for managing node sockets (input/output). These sockets serve as points of data connection between different nodes.

- **JSON Serialization**: The library provides functionalities to serialize/deserialize the node data to/from JSON format.

- **Integrated Copy/Paste**: With the help of JSON serialization/deserialization, elements can be copied from one node area to the same or another node area.
<div align="center">
    <img src="https://github.com/Azzinoth/VisualNodeSystem/blob/media/Copy and Paste.gif" width="60%">
</div>

- **Node Factory For Child Node Creation**: The library includes a system for creating child nodes using node factory, which enables JSON serialization of custom nodes.

- **Custom Context Menus**: The library supports the integration of custom context menus.

- **File System Integration**: The system allows you to save/load node data to/from files.

- **Node Event Callbacks**: The library provides functionalities to set callbacks for node events.

I have personally used it in two of my projects:

[Focal Engine Editor](https://github.com/Azzinoth/FocalEngineEditor)
![Focal Engine editor material window](https://github.com/Azzinoth/VisualNodeSystem/blob/media/Focal%20Engine%20Editor%20example.png)

[Focal Engine Test Platform](https://github.com/Azzinoth/FocalEngineTestPlatform)
![Focal Engine test platform](https://github.com/Azzinoth/VisualNodeSystem/blob/media/Test%20Platform%20example.png)

## Usage

**When using this project as a Git submodule, ensure you provide the path to Dear ImGui in your project. For a simple example of how to use library, see the [Visual Node System Example](https://github.com/Azzinoth/VisualNodeSystem-Example).**

To add this module to your project, use the following command:

```bash
git submodule add https://github.com/Azzinoth/VisualNodeSystem
```

If you want to move the submodule to a folder named "SubSystems", for example, use the following command:

```bash
git mv VisualNodeSystem SubSystems/
```

## Third Party Licenses

This project uses the following third-party libraries:

1) **GLM**: This library is licensed under a permissive open-source license, similar to the MIT license. The full license text can be found at [GLM's GitHub repository](https://github.com/g-truc/glm/blob/master/copying.txt).

2) **jsoncpp**: This library is licensed under the MIT License. The full license text can be found at [jsoncpp's GitHub repository](https://github.com/open-source-parsers/jsoncpp/blob/master/LICENSE).
