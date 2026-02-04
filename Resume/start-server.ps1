# Simple HTTP Server for Resume
$port = 8000
$url = "http://localhost:$port/"

# Create HTTP listener
$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add($url)
$listener.Start()

Write-Host "========================================" -ForegroundColor Green
Write-Host "Resume Server Started!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "Open your browser and go to: $url" -ForegroundColor Yellow
Write-Host "Press Ctrl+C to stop the server" -ForegroundColor Gray
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

# Open browser automatically
Start-Process $url

# MIME type mapping
function Get-MimeType {
    param([string]$filePath)
    $ext = [System.IO.Path]::GetExtension($filePath).ToLower()
    $mimeTypes = @{
        '.html' = 'text/html'
        '.htm' = 'text/html'
        '.css' = 'text/css'
        '.js' = 'application/javascript'
        '.jpg' = 'image/jpeg'
        '.jpeg' = 'image/jpeg'
        '.png' = 'image/png'
        '.gif' = 'image/gif'
        '.svg' = 'image/svg+xml'
        '.json' = 'application/json'
    }
    if ($mimeTypes.ContainsKey($ext)) {
        return $mimeTypes[$ext]
    }
    return 'application/octet-stream'
}

# Server loop
try {
    while ($listener.IsListening) {
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response
        
        $localPath = $request.Url.LocalPath
        if ($localPath -eq '/') {
            $localPath = '/index.html'
        }
        
        $filePath = Join-Path (Get-Location) $localPath.TrimStart('/')
        
        if (Test-Path $filePath -PathType Leaf) {
            $content = [System.IO.File]::ReadAllBytes($filePath)
            $response.ContentLength64 = $content.Length
            $response.ContentType = Get-MimeType $filePath
            $response.StatusCode = 200
            $response.OutputStream.Write($content, 0, $content.Length)
        } else {
            $response.StatusCode = 404
            $notFound = [System.Text.Encoding]::UTF8.GetBytes("404 - File Not Found")
            $response.ContentLength64 = $notFound.Length
            $response.ContentType = 'text/plain'
            $response.OutputStream.Write($notFound, 0, $notFound.Length)
        }
        
        $response.Close()
    }
} finally {
    $listener.Stop()
    Write-Host "`nServer stopped." -ForegroundColor Red
}
