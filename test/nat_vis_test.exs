defmodule ElixirNativeVisualizationTest do
  use ExUnit.Case
  doctest NatVis

  test "open_window and close_window should succeed" do
    window = NatVis.start
    :timer.sleep(1000)

    result = NatVis.stopRender window
    assert result == :ok
  end
end
