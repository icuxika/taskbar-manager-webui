if (!(Test-Path -Path .\third-party\webui)) {
    New-Item -ItemType Directory -Path .\third-party\webui
}
Invoke-WebRequest -Uri "https://github.com/webui-dev/webui/releases/download/2.4.2/webui-windows-msvc-x64.zip" -OutFile ".\third-party\webui\webui-windows-msvc-x64.zip"
Expand-Archive -LiteralPath ".\third-party\webui\webui-windows-msvc-x64.zip" -DestinationPath ".\third-party\webui"
Remove-Item -Path ".\third-party\webui\webui-windows-msvc-x64.zip"
Copy-Item -Recurse .\third-party\webui\webui-windows-msvc-x64\* .\third-party\webui\
Remove-Item -Recurse -Force .\third-party\webui\webui-windows-msvc-x64\