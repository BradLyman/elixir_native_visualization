defmodule ElixirNativeVisualizationTest do
  use ExUnit.Case
  doctest NatVis

  test "open_window should succeed" do
    assert NatVis.open_window() == :ok
  end
end
