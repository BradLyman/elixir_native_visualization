defmodule NatVis do
  @moduledoc """
  Documentation for ElixirNativeVisualization.
  """

  @on_load :load_nif

  def load_nif do
    :erlang.load_nif("_build/libNatVis", 0)
  end

  @doc """
  Open a window with SDL.
  """
  def open_window, do: raise "Not implemented!"

  def close_window(_window), do: raise "not implemented"

  def update(_geometry, _window), do: raise "not implemented"

  def run(window, frame) do
    event_loop(:ok, window, frame)
  end

  defp event_loop(:ok, window, frame) do
    frame.()
    |> update(window)
    |> event_loop(window, frame)
  end

  defp event_loop(:stop, window, _frame) do
    close_window window
  end

end

