import json
import subprocess
import sys
import tempfile
from pathlib import Path


def test_post_load_peak_rss():
    root = Path(__file__).resolve().parents[2]
    monitor = root / "experiment/post_load_peak_rss.py"
    with tempfile.TemporaryDirectory() as directory:
        input_path = Path(directory) / "input"
        output_path = Path(directory) / "rss.json"
        input_path.write_text("test\n")
        child = (
            "import sys,time; f=open(sys.argv[1]); time.sleep(.05); f.close(); "
            "x=bytearray(8_000_000); time.sleep(.05)"
        )
        subprocess.run(
            [sys.executable, monitor, "--input", input_path, "--output", output_path,
             "--timeout-seconds", "2", "--", sys.executable, "-c", child, input_path],
            check=True,
        )
        result = json.loads(output_path.read_text())
        assert result["input_fd_observed"]
        assert result["post_load_peak_rss_kib"] > 0
        assert not result["timed_out"]


if __name__ == "__main__":
    test_post_load_peak_rss()
