defmodule ElixirNativeVisualizationTest do
  use ExUnit.Case
  doctest NatVis

  test "open_window and close_window should succeed" do
    window = NatVis.open_window
    :timer.sleep(1000)

    result =
      window |> NatVis.close_window

    assert result == :ok
  end
end
