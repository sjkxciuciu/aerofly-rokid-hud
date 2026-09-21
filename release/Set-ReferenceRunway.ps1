param([string]$ConfigPath = (Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Aerofly FS 4\external_dll\AeroflyRokidApproach.ini'))
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
$form = New-Object Windows.Forms.Form
$form.Text = 'Aerofly HUD 2.5 - 3 degree reference / NOT ILS'
$form.ClientSize = New-Object Drawing.Size(620, 395)
$form.StartPosition = 'CenterScreen'
$form.FormBorderStyle = 'FixedDialog'
$form.MaximizeBox = $false
$labels = @('Runway name (e.g. ICAO / 09)', 'Threshold latitude (degrees, N positive)', 'Threshold longitude (degrees, E positive)', 'Threshold elevation (feet MSL)', 'Landing direction (TRUE degrees, 0-360)')
$keys = @('Name', 'Latitude', 'Longitude', 'ElevationFt', 'TrueCourse')
$inputs = @()
$existing = @{}
if (Test-Path -LiteralPath $ConfigPath) {
    foreach ($line in [IO.File]::ReadAllLines($ConfigPath)) {
        if ($line -match '^([A-Za-z]+)=(.*)$') { $existing[$matches[1]] = $matches[2] }
    }
}
for ($i = 0; $i -lt $labels.Count; $i++) {
    $label = New-Object Windows.Forms.Label
    $label.Text = $labels[$i]; $label.SetBounds(15, (20 + 45 * $i), 370, 25)
    $form.Controls.Add($label)
    $inputBox = New-Object Windows.Forms.TextBox
    $inputBox.SetBounds(390, (18 + 45 * $i), 210, 25)
    if ($existing.ContainsKey($keys[$i])) { $inputBox.Text = $existing[$keys[$i]] }
    $inputs += $inputBox; $form.Controls.Add($inputBox)
}
$note = New-Object Windows.Forms.Label
$note.Text = "Use the landing threshold, NOT airport center. Use TRUE, not magnetic course.`r`nAssumed path: 3 degrees, crossing threshold at 50 ft. Set aircraft QNH correctly.`r`nOnly for simulation. Save applies within ~1 second; no game restart required."
$note.SetBounds(15, 250, 590, 65); $form.Controls.Add($note)
$save = New-Object Windows.Forms.Button
$save.Text = 'Save / Enable'; $save.SetBounds(200, 335, 140, 35); $form.Controls.Add($save)
$disable = New-Object Windows.Forms.Button
$disable.Text = 'Disable reference'; $disable.SetBounds(365, 335, 150, 35); $form.Controls.Add($disable)
$save.Add_Click({
    try {
        if ($inputs[0].Text -notmatch '^[A-Za-z0-9 /-]{1,24}$' -or [string]::IsNullOrWhiteSpace($inputs[0].Text)) { throw 'Enter a runway name using 1-24 letters, numbers, spaces, / or -.' }
        $limits = @(@(-90,90), @(-180,180), @(-2000,20000), @(0,360))
        $lines = @('[Approach]', 'Enabled=1', ('Name=' + $inputs[0].Text))
        for ($i = 1; $i -lt 5; $i++) {
            $number = 0.0
            if (![double]::TryParse($inputs[$i].Text, [Globalization.NumberStyles]::Float, [Globalization.CultureInfo]::InvariantCulture, [ref]$number) -or
                [double]::IsNaN($number) -or [double]::IsInfinity($number) -or $number -lt $limits[$i-1][0] -or $number -gt $limits[$i-1][1]) {
                throw ('Invalid field: ' + $labels[$i] + '. Use a decimal point.')
            }
            $lines += $keys[$i] + '=' + $number.ToString('R', [Globalization.CultureInfo]::InvariantCulture)
        }
        $directory = Split-Path -Parent $ConfigPath
        if (!(Test-Path -LiteralPath $directory)) { throw 'Plugin directory not found. Install the HUD DLL first.' }
        # Same-directory atomic replacement: the reader never sees partially saved coordinates.
        $temporary = Join-Path $directory ('approach-' + [Guid]::NewGuid().ToString('N') + '.tmp')
        [IO.File]::WriteAllLines($temporary, $lines, [Text.Encoding]::ASCII)
        if (Test-Path -LiteralPath $ConfigPath) { [IO.File]::Replace($temporary, $ConfigPath, $null) }
        else { [IO.File]::Move($temporary, $ConfigPath) }
        [Windows.Forms.MessageBox]::Show('Reference runway saved. Check the runway name on the glasses.', 'Saved')
    } catch { [Windows.Forms.MessageBox]::Show($_.Exception.Message, 'Cannot save') }
})
$disable.Add_Click({
    try {
        if (Test-Path -LiteralPath $ConfigPath) {
            $lines = [IO.File]::ReadAllText($ConfigPath) -replace '(?m)^Enabled=.*$', 'Enabled=0'
            [IO.File]::WriteAllText($ConfigPath, $lines, [Text.Encoding]::ASCII)
        }
        [Windows.Forms.MessageBox]::Show('Reference disabled.', 'Disabled')
    } catch { [Windows.Forms.MessageBox]::Show($_.Exception.Message, 'Cannot disable') }
})
[void]$form.ShowDialog()
