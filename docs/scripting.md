# Scripting

The scripting project can be created inside or outside of the current project directory.
But engine will look the builded shared library at `{project path}/out/{project name}.dll`.

Here is an very basic example of an **.csproj** file which has a relative directory of
`{project path}/src/{project name}.csproj`
```xml
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>net8.0</TargetFramework>
    <OutputPath>..\out</OutputPath>
	<AppendTargetFrameworkToOutputPath>
		false
	</AppendTargetFrameworkToOutputPath>
  </PropertyGroup>
  <ItemGroup>
    <Reference Include="script_core">
      <HintPath>path\to\eve\script_core.dll</HintPath>
    </Reference>
  </ItemGroup>
</Project>
```
