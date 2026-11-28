param([Parameter(Mandatory = $true)][string]$OutputPath)
Add-Type -AssemblyName System.Drawing
$ErrorActionPreference = "Stop"
function New-VeyraIconPng([int]$Size) {
    $bitmap = New-Object System.Drawing.Bitmap($Size,$Size,[System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $graphics.Clear([System.Drawing.Color]::Transparent)
    $graphics.ScaleTransform([single]($Size / 256.0),[single]($Size / 256.0))
    $background = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255,15,27,59))
    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $path.AddArc(12,12,108,108,180,90); $path.AddArc(136,12,108,108,270,90)
    $path.AddArc(136,136,108,108,0,90); $path.AddArc(12,136,108,108,90,90)
    $path.CloseFigure(); $graphics.FillPath($background,$path)
    $orbitPen = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(190,95,145,205),3)
    $graphics.DrawEllipse($orbitPen,41,41,174,174)
    $arcPen = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(235,183,244,255),5)
    $arcPen.StartCap=[System.Drawing.Drawing2D.LineCap]::Round; $arcPen.EndCap=[System.Drawing.Drawing2D.LineCap]::Round
    $graphics.DrawArc($arcPen,45,45,166,166,205,130)
    $points=[System.Drawing.PointF[]]@([System.Drawing.PointF]::new(65,62),[System.Drawing.PointF]::new(113,181),[System.Drawing.PointF]::new(128,190),[System.Drawing.PointF]::new(143,181),[System.Drawing.PointF]::new(191,62),[System.Drawing.PointF]::new(152,62),[System.Drawing.PointF]::new(128,132),[System.Drawing.PointF]::new(104,62))
    $vBrush=New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255,75,195,245)); $graphics.FillPolygon($vBrush,$points)
    $node=New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255,124,235,255)); $graphics.FillEllipse($node,202,119,18,18)
    $stream=New-Object System.IO.MemoryStream; $bitmap.Save($stream,[System.Drawing.Imaging.ImageFormat]::Png); $bytes=$stream.ToArray(); $stream.Dispose()
    $node.Dispose();$vBrush.Dispose();$arcPen.Dispose();$orbitPen.Dispose();$background.Dispose();$path.Dispose();$graphics.Dispose();$bitmap.Dispose()
    return ,$bytes
}
$sizes=@(256,128,64,48,32,16);$images=@()
foreach($size in $sizes){$images+=,(New-VeyraIconPng $size)}
$stream=New-Object System.IO.MemoryStream;$writer=New-Object System.IO.BinaryWriter($stream)
$writer.Write([UInt16]0);$writer.Write([UInt16]1);$writer.Write([UInt16]$images.Count);$offset=6+(16*$images.Count)
for($i=0;$i -lt $images.Count;$i++){
 $size=$sizes[$i];$image=$images[$i];$dimension=if($size -eq 256){0}else{$size}
 $writer.Write([byte]$dimension);$writer.Write([byte]$dimension);$writer.Write([byte]0);$writer.Write([byte]0);$writer.Write([UInt16]1);$writer.Write([UInt16]32);$writer.Write([UInt32]$image.Length);$writer.Write([UInt32]$offset);$offset+=$image.Length
}
foreach($image in $images){$writer.Write($image)}
$writer.Flush();New-Item -ItemType Directory -Force -Path (Split-Path -Parent $OutputPath)|Out-Null
[System.IO.File]::WriteAllBytes($OutputPath,$stream.ToArray());$writer.Dispose();$stream.Dispose()
Write-Host "Generated Veyra icon: $OutputPath"
