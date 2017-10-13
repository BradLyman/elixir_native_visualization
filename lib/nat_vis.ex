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
    event_loop(:ok, :erlang.now, window, frame)
  end

  defp event_loop(:ok, lastFrame, window, frame) do

    consumed_time = :timer.now_diff(:erlang.now(), lastFrame)
    time_left = 15_000 - consumed_time
    time_as_ms = Integer.floor_div(time_left, 1000)
    IO.puts "Sleeping for #{time_as_ms}ms"
    :timer.sleep time_as_ms


    start = :erlang.now
    status = frame.() |> update(window)
    event_loop(status, start, window, frame)
  end

  defp event_loop(:stop, _lastFrame, window, _frame) do
    close_window window
  end

end

