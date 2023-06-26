# VisualNode System

![build](https://github.com/Azzinoth/VisualNodeSystem/actions/workflows/Build.yml/badge.svg?branch=master)

This library provides a powerful framework for creating and managing visual node systems. These are typically used in graphical programming environments, game logic builders, and AI behavior tree editors. I have personally used it in two of my projects:

[Focal Engine Editor](https://github.com/Azzinoth/FocalEngineEditor)
![Focal Engine editor material window](https://github.com/Azzinoth/VisualNodeSystem/blob/media/Focal%20Engine%20Editor%20example.png)

[Focal Engine Test Platform](https://github.com/Azzinoth/FocalEngineTestPlatform)
![Focal Engine test platform](https://github.com/Azzinoth/VisualNodeSystem/blob/media/Test%20Platform%20example.png)

## Key Features

- **Versatile Node Styles**: This library supports various visual node styles, currently including Default and Circle. You can easily expand this with new styles.

- **Flexible Socket Management**: The library offers functionalities for managing node sockets (input/output). These sockets serve as points of data connection between different nodes.

- **Visual Node Area Management**: This feature allows you to manage a visual node area, offering several operations like getting selected nodes, connecting nodes, rendering, and event propagation.

- **JSON Serialization**: The library provides functionalities to serialize/deserialize the node data to/from JSON format.

- **Macro for Child Node Creation**: The library includes a system for creating child nodes with macros, which enables JSON serialization of custom nodes.

- **Custom Context Menus**: The library supports the integration of custom context menus.

- **File System Integration**: The system allows you to save/load node data to/from files.

- **Node Event Callbacks**: The library provides functionalities to set callbacks for node events.

- **Area Properties**: You can get and set various area properties like area position, size, and render offset.

## Usage

For a simple example of how to use library, see the [VisualNodeSystem Example](https://github.com/Azzinoth/VisualNodeSystem-Example).

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